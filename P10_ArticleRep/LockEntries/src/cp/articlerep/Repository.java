package cp.articlerep;

import java.util.HashSet;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import cp.articlerep.ds.Map;
import cp.articlerep.ds.HashTable;

/**
 * @author Ricardo Dias
 */
public class Repository {

	private Map<String, List<Article>> byAuthor;
	private Map<String, List<Article>> byKeyword;
	private Map<Integer, Article> byArticleId;

	public Repository(int nkeys) {
		this.byAuthor = new HashTable<String, List<Article>>(40000);
		this.byKeyword = new HashTable<String, List<Article>>(40000);
		this.byArticleId = new HashTable<Integer, Article>(40000);
	}

	public boolean insertArticle(Article a) {

		if (byArticleId.contains(a.getId()))
			return false;

                byArticleId.lockKey(a.getId());
                Iterator<String> auIt = a.getAuthors().iterator();
                while(auIt.hasNext())
                    byAuthor.lockKey(auIt.next());

                Iterator<String> kwIt = a.getKeywords().iterator();
                while(kwIt.hasNext())
                    byKeyword.lockKey(kwIt.next());


		Iterator<String> authors = a.getAuthors().iterator();
		while (authors.hasNext()) {
			String name = authors.next();

			List<Article> ll = byAuthor.get(name);
			if (ll == null) {
				ll = new LinkedList<Article>();
				byAuthor.put(name, ll);
			}
			ll.add(a);
		}

		Iterator<String> keywords = a.getKeywords().iterator();
		while (keywords.hasNext()) {
			String keyword = keywords.next();

			List<Article> ll = byKeyword.get(keyword);
			if (ll == null) {
				ll = new LinkedList<Article>();
				byKeyword.put(keyword, ll);
			} 
			ll.add(a);
		}

		byArticleId.put(a.getId(), a);

                byArticleId.unlockKey(a.getId());
                auIt = a.getAuthors().iterator();
                while(auIt.hasNext())
                    byAuthor.unlockKey(auIt.next());

                kwIt = a.getKeywords().iterator();
                while(kwIt.hasNext())
                    byKeyword.unlockKey(kwIt.next());

		return true;
	}

	public void removeArticle(int id) {
		Article a = byArticleId.get(id);

		if (a == null)
			return;

                byArticleId.lockKey(a.getId());

                Iterator<String> authorIt = a.getAuthors().iterator();
                while(authorIt.hasNext())
                    byAuthor.lockKey(authorIt.next());

                Iterator<String> keywordIt = a.getKeywords().iterator();
                while(keywordIt.hasNext())
                    byKeyword.lockKey(keywordIt.next());

                //Start remotion

		byArticleId.remove(id);

		Iterator<String> keywords = a.getKeywords().iterator();
		while (keywords.hasNext()) {
			String keyword = keywords.next();

			List<Article> ll = byKeyword.get(keyword);
			if (ll != null) {
				int pos = 0;
				Iterator<Article> it = ll.iterator();
				while (it.hasNext()) {
					Article toRem = it.next();
					if (toRem == a) {
						break;
					}
					pos++;
				}
				ll.remove(pos);
				it = ll.iterator();
				if (!it.hasNext()) { // checks if the list is empty
					byKeyword.remove(keyword);
				}
			}
		}

		Iterator<String> authors = a.getAuthors().iterator();
		while (authors.hasNext()) {
			String name = authors.next();

			List<Article> ll = byAuthor.get(name);
			if (ll != null) {
				int pos = 0;
				Iterator<Article> it = ll.iterator();
				while (it.hasNext()) {
					Article toRem = it.next();
					if (toRem == a) {
						break;
					}
					pos++;
				}
				ll.remove(pos);
				it = ll.iterator(); 
				if (!it.hasNext()) { // checks if the list is empty
					byAuthor.remove(name);
				}
			}
		}

                byArticleId.unlockKey(a.getId());

                authorIt = a.getAuthors().iterator();
                while(authorIt.hasNext())
                    byAuthor.unlockKey(authorIt.next());

                keywordIt = a.getKeywords().iterator();
                while(keywordIt.hasNext())
                    byKeyword.unlockKey(keywordIt.next());
	}

	public List<Article> findArticleByAuthor(List<String> authors) {
		List<Article> res = new LinkedList<Article>();

                Iterator<String> authorIt = authors.iterator();
                while(authorIt.hasNext())
                    byAuthor.lockKey(authorIt.next());


		Iterator<String> it = authors.iterator();
		while (it.hasNext()) {
			String name = it.next();
			List<Article> as = byAuthor.get(name);
			if (as != null) {
				Iterator<Article> ait = as.iterator();
				while (ait.hasNext()) {
					Article a = ait.next();
					res.add(a);
				}
			}
		}
                
                authorIt = authors.iterator();
                while(authorIt.hasNext())
                    byAuthor.unlockKey(authorIt.next());
    

		return res;
	}

	public List<Article> findArticleByKeyword(List<String> keywords) {
		List<Article> res = new LinkedList<Article>();

                Iterator<String> keywordIt = keywords.iterator();
                while(keywordIt.hasNext())
                    byAuthor.lockKey(keywordIt.next());

		Iterator<String> it = keywords.iterator();
		while (it.hasNext()) {
			String keyword = it.next();
			List<Article> as = byKeyword.get(keyword);
			if (as != null) {
				Iterator<Article> ait = as.iterator();
				while (ait.hasNext()) {
					Article a = ait.next();
					res.add(a);
				}
			}
		}

                keywordIt = keywords.iterator();
                while(keywordIt.hasNext())
                    byKeyword.unlockKey(keywordIt.next());
		
                return res;
	}

	
	/**
	 * This method is supposed to be executed with no concurrent thread
	 * accessing the repository.
	 * 
	 */
	public boolean validate() {
		
		HashSet<Integer> articleIds = new HashSet<Integer>();
		int articleCount = 0;
		
		Iterator<Article> aIt = byArticleId.values();
		while(aIt.hasNext()) {
			Article a = aIt.next();
			
			articleIds.add(a.getId());
			articleCount++;
			
			// check the authors consistency
			Iterator<String> authIt = a.getAuthors().iterator();
			while(authIt.hasNext()) {
				String name = authIt.next();
				if (!searchAuthorArticle(a, name)) {
					return false;
				}
			}
			
			// check the keywords consistency
			Iterator<String> keyIt = a.getKeywords().iterator();
			while(keyIt.hasNext()) {
				String keyword = keyIt.next();
				if (!searchKeywordArticle(a, keyword)) {
					return false;
				}
			}
		}
		
		return articleCount == articleIds.size();
	}
	
	private boolean searchAuthorArticle(Article a, String author) {
		List<Article> ll = byAuthor.get(author);
		if (ll != null) {
			Iterator<Article> it = ll.iterator();
			while (it.hasNext()) {
				if (it.next() == a) {
					return true;
				}
			}
		}
		return false;
	}

	private boolean searchKeywordArticle(Article a, String keyword) {
		List<Article> ll = byKeyword.get(keyword);
		if (ll != null) {
			Iterator<Article> it = ll.iterator();
			while (it.hasNext()) {
				if (it.next() == a) {
					return true;
				}
			}
		}
		return false;
	}

}
